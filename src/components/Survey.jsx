import React, { useState } from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect, isEmpty } from 'react-redux-firebase'
import firebase from '../firebaseConfigs'
import { css } from 'glamor'

import yesIcon from '../static/img/check.svg'
import noIcon from '../static/img/close.svg'

const SAVING = false
const THANKS_DELAY = 1500

export const Vote = props => {
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
			{...css({
				position: 'absolute',
				top: '3%',
				left: '3%',
				width: '94%',
				height: '50%',
				[`@media(min-width: ${theme.breaks.md}px)`]: {
					height: '40%',
				},
				display: 'flex',
				alignItems: 'center',
				background: 'rgba(255, 255, 255, 0.8)',
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
	const theme = useSelector(state => state.theme) || []

	useFirebaseConnect([
		{
			path: 'fortunes',
			storeAs: 'fortunesLowVotes',
			queryParams: ['orderByChild=rank/total', 'limitToFirst=3'],
		},
	])
	const fortunesLowVotes = useSelector(state => state.firebase.data.fortunesLowVotes) || []

	let selected = false
	let fortunes = []

	if (isEmpty(fortunesLowVotes)) {
		console.log('Loading...')
	} else {
		Object.keys(fortunesLowVotes).forEach((k, i) => {
			fortunes.push({ ...fortunesLowVotes[k], id: k })
		})

		selected = fortunes[Math.floor(Math.random() * fortunes.length)]
	}

	const [forced, forceRefresh] = useState(false)

	const handleVote = e => {
		if (SAVING) {
			firebase
				.database()
				.ref()
				.update({
					[`/fortunes/${selected.id}/rank/total`]: selected.rank ? selected.rank.total + 1 : 1,
				})

			if (parseInt(e.currentTarget.value) === 1) {
				firebase
					.database()
					.ref()
					.update({
						[`/fortunes/${selected.id}/rank/keep`]:
							selected.rank && selected.rank.keep ? selected.rank.keep + 1 : 1,
					})
			}
		}
		forceRefresh(true)
		setTimeout(() => {
			forceRefresh(false)
		}, THANKS_DELAY)
	}

	const voteBtn = css({
		fontFamily: `'VT323', Roboto, Helvetica, Arial, sans-serif`,
		textAlign: 'center',
		fontSize: '3rem',
		padding: '1rem 2rem',
		margin: '0 1rem',
		[`@media(min-width: ${theme.breaks.md}px)`]: {
			margin: '1rem 3rem',
		},
		borderRadius: theme.borerRadius,
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
			{fortunes.length && selected ? (
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

					<Vote value={1} {...css({ gridArea: 'yes' })} {...voteBtn} onClick={handleVote} />
					<Vote
						value={-1}
						{...css({ gridArea: 'nope', '& img': { transform: 'scale(0.9)' } })}
						{...voteBtn}
						onClick={handleVote}
					/>
					{forced && <Thanks />}
				</React.Fragment>
			) : (
				<h4>Loading</h4>
			)}
		</div>
	)
}
