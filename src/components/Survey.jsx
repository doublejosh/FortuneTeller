import React, { useState } from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect } from 'react-redux-firebase'
import { css } from 'glamor'

import yesIcon from '../static/img/check.svg'
import noIcon from '../static/img/close.svg'

export const Vote = props => {
	const label = props.value > 0 ? 'Yes' : 'No'

	const handleVote = e => {
		props.forceRefresh({})
		console.log('VOTE' + label)
		e.currentTarget.blur()
	}

	return (
		<button onClick={handleVote} {...props}>
			<img src={props.value > 0 ? yesIcon : noIcon} alt={label} /> <span>{label}</span>
		</button>
	)
}

export default () => {
	const theme = useSelector(state => state.theme) || []

	useFirebaseConnect(['fortunes'])
	let fortunes = useSelector(state => state.firebase.data.fortunes) || []
	fortunes = fortunes.filter(x => x)
	const selected = fortunes[Math.floor(Math.random() * fortunes.length)]

	const [forced, forceRefresh] = useState(true)

	const voteBtn = css({
		textAlign: 'center',
		fontSize: '3rem',
		padding: '1rem 2rem',
		margin: '0 1rem',
		[`@media(min-width: ${theme.breaks.md}px)`]: {
			margin: '1rem 3rem',
		},
		borderRadius: '10rem',
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
			{fortunes.length ? (
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

					<Vote
						value={1}
						{...css({ gridArea: 'yes' })}
						{...voteBtn}
						onClick={() => forceRefresh(!forced)}
					/>
					<Vote
						value={-1}
						{...css({ gridArea: 'nope', '& img': { transform: 'scale(0.9)' } })}
						{...voteBtn}
						onClick={() => forceRefresh(!forced)}
					/>
				</React.Fragment>
			) : (
				<h4>Loading</h4>
			)}
		</div>
	)
}
