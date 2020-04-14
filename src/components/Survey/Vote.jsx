import React, { useState, useMemo } from 'react'
import { useHistory } from 'react-router'
import { useSelector } from 'react-redux'
import { useFirebaseConnect, useFirebase, isEmpty } from 'react-redux-firebase'
import { CheckCircle as IconCheckCircle, NoBox as IconNoBox } from '../Icons'
import { css } from 'glamor'

const SAVING = true
const THANKS_DELAY = 1500

export const Button = props => {
	const label = props.value > 0 ? 'Yes' : 'No'
	return (
		<button onClick={props.onClick} {...props}>
			{props.value > 0 ? <IconCheckCircle /> : <IconNoBox />}
			<span>{label}</span>
		</button>
	)
}

export const Thanks = props => {
	return (
		<div id="thanks" {...css(props.css)}>
			<h3>Got it.</h3>
		</div>
	)
}

export default props => {
	const firebase = useFirebase()

	const prepRankable = list =>
		Object.keys(list)
			.filter(f => list[f])
			.map(f => ({ ...list[f], key: f }))

	const query = {
		type: 'once', // Avoid other user votes.
		path: 'fortunes',
		storeAs: 'fortunesLowVotes',
		queryParams: ['orderByChild=rank/lastVote', 'limitToFirst=1'],
	}
	// http://react-redux-firebase.com/docs/queries#types-of-queries
	useFirebaseConnect([query])
	const fortunesLowVotes = useSelector(state => state.firebase.data.fortunesLowVotes)
	const [selected, setSelected] = useState({})

	const history = useHistory()

	useMemo(() => {
		const list = prepRankable(fortunesLowVotes || {})
		setSelected(list.length ? list[Math.floor(Math.random() * list.length)] : {})
	}, [fortunesLowVotes])

	const handleVote = (f, e) => {
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
						lastVote: firebase.database.ServerValue.TIMESTAMP,
					},
				})
		} else console.log('SAVING DISABLED')

		document.getElementById('thanks').style.display = 'flex'
		setTimeout(() => {
			document.getElementById('thanks').style.display = 'none'
			setSelected({})
			history.go()
			//firebase.watchEvent('once', 'fortunesLowVotes')
			//window.location.reload(false)
		}, THANKS_DELAY)
	}

	const theme = useSelector(state => state.theme) || []
	const buttonStyle = css({
		borderRadius: theme.borderRadius,
		background: 'rgba(255, 255, 255, .05)',
		lineHeight: '1.5rem',
		userSelect: 'none',
		border: 'none',
		outline: 0,
		fontFamily: `'VT323', Roboto, Helvetica, Arial, sans-serif`,
		textAlign: 'center',
		fontSize: '2.5rem',
		margin: '0 1rem',
		color: '#FFF',
		tapHighlightColor: '#000',
		'&:active': {
			color: '#000',
			'& path': {
				fill: '#000',
			},
			background: 'rgba(255, 255, 255, .1)',
		},
		'@media (hover: hover) and (pointer: fine)': {
			'&:hover': {
				color: theme.background,
				'& path': {
					fill: theme.background,
				},
				background: 'rgba(255, 255, 255, .7)',
			},
			'&:active': {
				color: '#000',
				'& path': {
					fill: '#000',
				},
				background: 'rgba(255, 255, 255, 1)',
			},
		},
		padding: '1rem',
		[`@media(min-width: ${theme.breaks.md}px)`]: {
			padding: '2rem',
			margin: '1rem 3rem',
			fontSize: '3rem',
			'& img': {
				maxWidth: '5rem',
			},
		},
		'& svg': {
			maxWidth: '3rem',
			maxHeight: '3rem',
			[`@media(min-width: ${theme.breaks.md}px)`]: {
				maxWidth: '6rem',
				maxHeight: '6rem',
			},
		},
		'& path': {
			fill: '#FFF',
		},
		'& img': {
			maxWidth: '4rem',
		},
		'& span': {
			display: 'block',
			marginTop: '.5rem',
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
				gridTemplateRows: '55% 1fr 10rem',
				[`@media(min-width: ${theme.breaks.md}px)`]: {
					gridTemplateRows: '50% 1fr 15rem',
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
							fontSize: '2rem',
							lineHeight: '2.5rem',
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
						{...buttonStyle}
						onClick={e => handleVote(selected, e)}
					/>
					<Button
						value={-1}
						{...css({ gridArea: 'nope' })}
						{...buttonStyle}
						onClick={e => handleVote(selected, e)}
					/>
					<Thanks
						id="thanks"
						css={{
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
							background: 'rgba(255, 255, 255, 0.8)',
							color: '#000',
							borderRadius: `${theme.borderRadius}`,
							fontSize: '5rem',
							justifyContent: 'center',
						}}
					/>
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
