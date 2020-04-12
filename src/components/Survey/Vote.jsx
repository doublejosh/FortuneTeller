import React, { useState, useMemo, useCallback, useEffect } from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect, useFirebase, isEmpty } from 'react-redux-firebase'
import firebase from '../../firebaseConfigs'
import { css } from 'glamor'

import yesIcon from '../../static/img/check.svg'
import noIcon from '../../static/img/close.svg'

const SAVING = true
const THANKS_DELAY = 1500

export const Button = props => {
	const label = props.value > 0 ? 'Yes' : 'No'
	return (
		<button onClick={props.onClick} {...props}>
			<img src={props.value > 0 ? yesIcon : noIcon} alt={label} /> <span>{label}</span>
		</button>
	)
}

export const Thanks = () => {
	const theme = useSelector(state => state.theme) || []
	return (
		<div
			id="thanks"
			{...css({
				position: 'absolute',
				top: '3%',
				left: '3%',
				width: '94%',
				height: '50%',
				[`@media(min-width: ${theme.breaks.md}px)`]: {
					height: '40%',
				},
				display: 'none',
				//display: 'flex',
				alignItems: 'center',
				background: 'rgba(255, 255, 255, 0.5)',
				color: '#000',
				borderRadius: `${theme.borderRadius}`,
				fontSize: '5rem',
				justifyContent: 'center',
			})}>
			<h3>Got it.</h3>
		</div>
	)
}

export default () => {
	const firebase = useFirebase()

	console.log('RENDER ---------------------------------------------')

	const prepRankable = list =>
		Object.keys(list)
			.filter(f => list[f])
			.map(f => ({ ...list[f], key: f }))

	// http://react-redux-firebase.com/docs/queries#types-of-queries
	const query = {
		type: 'once', // Avoid other user votes.
		path: 'fortunes',
		storeAs: 'fortunesLowVotes',
		queryParams: ['orderByChild=rank/total', 'limitToFirst=1'],
	}
	useFirebaseConnect([query])

	const fortunesLowVotes = useSelector(state => state.firebase.data.fortunesLowVotes)

	const [selected, setSelected] = useState({})
	const [force, forceUpdate] = useState(0)

	console.log('RAW QUERY')
	console.log(fortunesLowVotes)

	useMemo(() => {
		const list = prepRankable(fortunesLowVotes || {})

		console.log('MEMOIZED LIST')
		console.log(list)

		setSelected(list.length ? list[Math.floor(Math.random() * list.length)] : {})
	}, [fortunesLowVotes, force])

	console.log('PICKED')
	console.log(selected)

	//useEffect(() => firebase.watchEvent('child_changed', 'fortunesLowVotes'))

	const handleVote = (f, e) => {
		console.log('CLICK ****************************************')

		const btnVal = parseInt(e.currentTarget.value)
		const nextKeep =
			btnVal === 1
				? f.rank && f.rank.keep && f.rank.keep > 0
					? f.rank.keep + 1
					: 1
				: f.rank && f.rank.keep
				? f.rank.keep
				: 0

		if (SAVING) {
			firebase
				.database()
				.ref()
				.update({
					[`/fortunes/${selected.key}/rank`]: {
						total: selected.rank ? selected.rank.total + 1 : 1,
						keep: nextKeep,
					},
				})
		} else console.log('SAVING DISABLED')

		document.getElementById('thanks').style.display = 'flex'
		setTimeout(() => {
			document.getElementById('thanks').style.display = 'none'
			setSelected({})
			forceUpdate(!force)
			//firebase.watchEvent('once', 'fortunesLowVotes')
			window.location.reload(false)
		}, THANKS_DELAY)
	}

	const theme = useSelector(state => state.theme) || []
	const voteBtn = css({
		fontFamily: `'VT323', Roboto, Helvetica, Arial, sans-serif`,
		textAlign: 'center',
		fontSize: '3rem',
		padding: '1rem 2rem',
		margin: '0 1rem',
		[`@media(min-width: ${theme.breaks.md}px)`]: {
			margin: '1rem 3rem',
		},
		borderRadius: theme.borderRadius,
		background: 'rgba(255, 255, 255, .05)',
		color: '#000',
		lineHeight: '1.5rem',
		userSelect: 'none',
		border: 'none',
		outline: 0,
		'& img': {
			maxWidth: '5rem',
		},
		'& span': {
			display: 'block',
		},
	})

	return (
		<div
			{...css({
				display: 'grid',
				gridTemplateAreas: `"message message"
                            "nope yes"
                            "space space"`,
				gridTemplateColumns: '1fr 1fr',
				gridTemplateRows: '65% 1fr 5%',
				[`@media(min-width: ${theme.breaks.md}px)`]: {
					gridTemplateRows: '50% 1fr 10%',
				},
				minHeight: '100%',
				padding: '0 1rem',
			})}>
			{!isEmpty(selected) ? (
				<React.Fragment>
					<div
						{...selected}
						{...css({
							gridArea: 'message',
							display: 'flex',
							alignItems: 'center',
							justifyContent: 'center',
							padding: '0 1rem',
							fontSize: '2.25rem',
							[`@media(min-width: ${theme.breaks.md}px)`]: {
								fontSize: '2rem',
								padding: '0 2rem',
							},
							'& h2': {
								margin: '0 auto',
								textAlign: 'center',
							},
						})}>
						<h2>{selected.text}</h2> {selected.score}
					</div>

					<Button
						value={1}
						{...css({ gridArea: 'yes' })}
						{...voteBtn}
						onClick={e => handleVote(selected, e)}
					/>
					<Button
						value={-1}
						{...css({ gridArea: 'nope', '& img': { transform: 'scale(0.9)' } })}
						{...voteBtn}
						onClick={e => handleVote(selected, e)}
					/>
					<Thanks id="thanks" />
				</React.Fragment>
			) : (
				<h2
					{...css({
						gridArea: 'message',
						display: 'flex',
						alignItems: 'center',
						justifyContent: 'center',
						animation: 'blinkingText 1.2s infinite',
					})}>
					Loading...
				</h2>
			)}
		</div>
	)
}
